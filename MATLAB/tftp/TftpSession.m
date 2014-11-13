classdef TftpSession < Tftp
    %TftpSession 
    %   
    
    properties
        id = char([]);
        records = TftpRecord.empty;
        attributes = {};
    end
    
    methods
        % TftpSession constructor
        function obj = TftpSession(id)
            % parse input
            p = inputParser;
            addRequired(p, 'id', @ischar);
            parse(p, id);
            
            % set properties
            obj.id = p.Results.id;
            
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
        
        % push()
        function obj = push(obj, code, times, values)
            % push values
            obj.records(code) = obj.records(code).push(times, values);
        end
        
        % store
        % -----
        % save segment data to session
        %
        function session = store(obj, segments)
            % process all segments
            for i = 1:length(segments)
                % get segment
                segment = segments(i);
               
                % push data to record
                session = obj.push(...
                    segment.code,...
                    segment.time,...
                    segment.value...
                );
            end
        end
    end
    
end

