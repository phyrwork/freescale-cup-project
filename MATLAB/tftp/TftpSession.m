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
    end
    
end

