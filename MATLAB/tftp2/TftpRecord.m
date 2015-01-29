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
        function obj = push(obj,entries)
            
            % check allocation
            l = length(obj.times);
            e = length(entries);
            persistent last_rem;                              % buffer remainder on last call
            this_rem = rem(l+e,obj.prealloc); % buffer remainder after this call
            
            if isempty(last_rem) || this_rem < last_rem % if remainder set to 'decrease' then buffer is going to overflow
                
                % extend times
                tmp = obj.times;                              % make copy of data
                obj.times = zeros(0,l+obj.prealloc,'single'); % allocate a larger block
                obj.times(1:l) = tmp;                         % write data back        
                
                tmp = obj.values;
                obj.values = zeros(0,l+obj.prealloc,obj.type);
                obj.values(1:l) = tmp;
            end
            
            last_rem = this_rem;
            
            obj.times(end+1:end+e) = [entries.time];
            obj.values(end+1:end+e) = [entries.value];
        end
    end
    
end

