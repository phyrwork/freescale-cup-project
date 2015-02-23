classdef TftpSession < Tftp
    %TftpSession 
    %   
    
    properties
        id = char([]);
        device = serial.empty;
        records = TftpRecord.empty;
        attributes = {};
    end
    
    %
    % Initialization methods
    %
    methods
        % TftpSession constructor
        function obj = TftpSession(id, varargin)
            % parse input
            p = inputParser;
            addRequired(p, 'id', @ischar);
            addOptional(p, 'device', []);
            parse(p, id, varargin{:});
            
            % set properties
            obj.id = p.Results.id;
            obj.device = p.Results.device;
            
            % instantiate records
            for i = 1:length(obj.modules)
                module = obj.modules{i};
                if ( isempty(module) ) continue; end;
                obj = obj.addRecord(module);
            end
        end
        
        % addRecord
        function obj = addRecord(obj, module)
            % instantiate record
            obj.records(module.code) = TftpRecord(...
                module.attribute,...
                module.mtype,...
                module.ssize...
            );
            % add attribute ID to cell array
            obj.attributes{module.code} = module.attribute;
        end
    end
    
    %
    % Methods for collecting data
    %
    methods  
        % store
        % -----
        % save segment data to session
        %
        function obj = store(obj, segments)
            % process all segments
            for i = 1:length(segments)
                % push data to record
                obj.records(segments(i).code) = ...
                    obj.records(segments(i).code).push(...
                        segments(i).time,...
                        segments(i).value...
                    );
            end
        end
        
        % receive
        % -----
        % fetch data from serial; parse; store
        %
        function obj = receive(obj)
            % get frames from serial port
            raw = SerialReceive(obj.device);
            
            % decode frames
            frames = {};
            numFrames = 1;
            for i = 1:length(raw)
                try
                    frames{numFrames} = SerialDecode(raw{i});
                    numFrames = numFrames + 1;
                catch
                    disp('Frame discarded.');
                end
            end
            
            % no frames, no data; return
            if (isempty(frames))
                disp('No frames.');
                return;
            else
                disp([num2str(length(frames)),' new frames...']);
            end
            
            % split into samples
            groups = [];
            for i = 1:length(frames)
                try
                    groups{i} = obj.parse(frames{i});
                catch
                    disp('Failed to parse frame.')
                end
            end
            
            % store samples from groups
            numSamples = 0;
            if (~isempty(groups))
                for i = 1:length(groups)
                    if (~isempty(groups{i}))
                        numSamples = numSamples + length(groups{i});
                        for j = 1:length(groups{i})
                            obj = obj.store(groups{i}(j));
                        end
                    end
                end
            end
            disp(['...', num2str(numSamples), ' new samples.']);
        end
    end
    
    %
    % Methods for accessing data contained in child TftpRecords
    %
    methods
        % afterTime
        function [times, values] = afterTime(obj, attribute, time)
            record = obj.findAttribute(attribute);
            if (isempty(record))
                error('Attribute not found in this session.');
            end
            [times, values] = record.afterTime(time);
        end
        
        % atTime
        function [time, value] = atTime(obj, attribute, time)
            record = obj.findAttribute(attribute);
            if (isempty(record))
                error('Attribute not found in this session.');
            end
            [time, value] = record.atTime(time);
        end
    end
   
    
    %
    % Support methods
    %
    methods
        function record = findAttribute(obj, attribute)
            % search through array of attribute strings to find index of
            % attribute
            for (ind = 1:length(obj.attributes))
                if (strcmp(attribute, obj.attributes{ind}))
                    record = obj.records(ind);
                    return;
                end
            end
            record = [];
        end
    end
end

