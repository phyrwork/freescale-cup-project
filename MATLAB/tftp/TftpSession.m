classdef TftpSession < Tftp
    %TftpSession 
    %   
    
    properties
        id = char([]);
        device = serial.empty;
        records = TftpRecord.empty;
        attributes = {};
    end
    
    methods
        % TftpSession constructor
        function obj = TftpSession(id, device)
            % parse input
            p = inputParser;
            addRequired(p, 'id', @ischar);
            addRequired(p, 'device');
            parse(p, id, device);
            
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
        end
        
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
            numFrames = 1;
            for i = 1:length(raw)
                %try
                    frames{numFrames} = SerialDecode(raw{i});
                    numFrames = numFrames + 1;
                %catch
                %    disp('Frame discarded.');
                %end
            end
            
            % split into samples
            for i = 1:length(frames)
                groups{i} = obj.parse(frames{i});
            end
            
            % store samples from groups
            for i = 1:length(groups)
                for j = 1:length(groups{i})
                    obj = obj.store(groups{i}(j));
                end
            end
        end
    end
    
end

