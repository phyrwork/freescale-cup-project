classdef TftpSession < TftpCore % & TftpLink
    %TftpSession
    
    properties
        records = {};
    end
    
    methods
        % constructor
        function obj = TftpSession()
            
            % initialise Tftp
            obj = obj@TftpCore;
            
            % configure link
            %obj = obj@TftpLink();

            % instantiate records
            ind = cellfun('isempty',obj.modules);
            ind = find(ind == 0);
            for i = ind;
                obj = obj.addRecord(obj.modules{i}, i);
            end
        end
        
        % addRecord
        function obj = addRecord(obj, module, index)
            
            % instantiate record
            obj.records{index} = TftpRecord( ...
                module.attribute, ...
                module.mtype ...
            );
        end
        
        
        % receive
        function obj = update(obj, frames)
            
            % receive messages from link
            %frames = obj.receive;
            
            % separate into samples
            for i = 1:length(frames)
                try
                    entries{i} = obj.parse(frames{i});
                catch
                    disp('Error parsing frame.')
                end
            end
            
            entries = [entries{:}];
            
            % store samples for one record at a time
            rind = cellfun('isempty',obj.records);
            rind = find(rind == 0);
            for i = rind;
                eind = [entries.code] == i;
                obj.records{i} = obj.records{i}.push(entries(eind));
            end
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

