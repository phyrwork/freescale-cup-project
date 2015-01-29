classdef TftpRecord
    %TftpRecord 
    
    properties (Constant)
        prealloc = 2^16; % extend in 65536 sample blocks
    end
    
    properties
        attribute = char.empty;
        times;
        values;
        type = char.empty;
    end
    
    methods
        % constructor
        function obj = TftpRecord(attribute, type)
            
            % parse arguments
            p = inputParser;
            addRequired(p, 'attribute', @ischar);
            addRequired(p, 'type',      @ischar);
            parse(p, attribute, type);
            
            % initialise record
            obj.attribute = p.Results.attribute;
            obj.type = p.Results.type; 
        end
        
        % push entries
        function obj = push(obj,entry)
              
            % check allocation
            l = length(obj.times);
            if rem(l,obj.prealloc) == 0 % if record is full
                
                % extend times
                tmp = obj.times;                              % make copy of data
                obj.times = zeros(0,l+obj.prealloc,'single'); % allocate a larger block
                obj.times(1:l) = tmp;                         % write data back        
                
                tmp = obj.values;
                obj.values = zeros(0,l+obj.prealloc,obj.type);
                obj.values(1:l) = tmp;
            end
            
            obj.times(end+1) = entry.time;
            obj.values(end+1) = entry.value;
        end
    end
    
end

