classdef TftpSegment
    %TftpSegment
    
    properties
        code = cast([], 'uint8');
        time;
        value;
    end
    
    methods
        % TftpSegment constructor
        function obj = TftpSegment(code, time, value);
            obj.code = code;
            obj.time = time;
            obj.value = value;
        end
    end
end

