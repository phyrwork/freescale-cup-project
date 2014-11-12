classdef TftpSession
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
        end
        
        % lookup
        function index = lookup(obj, attribute)
            % search list of records for attribute
            index = strmatch(attribute, obj.attributes, 'exact');
            if ( isempty(index) ) index = 0; end;
        end
        
        % addRecord
        function obj = addRecord(obj, attribute)
            obj.attributes = [obj.attributes, attribute];
            obj.records(length(obj.attributes)) = TftpRecord(attribute);
            
        end
        
        % push()
        function obj = push(obj, attribute, times, values)
            % validate input
            if ( ~ischar(attribute) )
                error('Attribute identifier must be of type char()');
            end
            
            % search for attribute
            index = obj.lookup(attribute);
            if (index == 0) % if record for attribute doesn't exist
                obj = obj.addRecord(attribute);
                index = length(obj.records);
            end
            
            % push values
            obj.records(index) = obj.records(index).push(times, values);
        end
    end
    
end

