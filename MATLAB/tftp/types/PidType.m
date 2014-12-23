classdef PidType
    %PidType
    
    properties
        Kp        = single.empty;
        Ki        = single.empty;
        Kd        = single.empty;
        time      = single.empty;
        value     = single.empty;
        value_max = single.empty;
        value_min = single.empty;
        error     = single.empty;
        integral  = single.empty;
    end

    methods
        % PidType constructor
        function obj = PidType(varargin)
            % Check no. args
            if (nargin == 0)
                return;
            end
            
            % Unpack into properties
            obj.Kp        = typecast(flip(varargin{1}(1:4)), 'single');
            obj.Ki        = typecast(flip(varargin{1}(5:8)), 'single');
            obj.Kd        = typecast(flip(varargin{1}(9:12)), 'single');
            obj.time      = typecast(flip(varargin{1}(13:16)), 'single');
            obj.value     = typecast(flip(varargin{1}(17:20)), 'single');
            obj.value_max = typecast(flip(varargin{1}(21:24)), 'single');
            obj.value_min = typecast(flip(varargin{1}(25:28)), 'single');
            obj.error     = typecast(flip(varargin{1}(29:32)), 'single');
            obj.integral  = typecast(flip(varargin{1}(33:36)), 'single');
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1:4)   = flip(typecast(obj.Kp, 'uint8'));
            stream(5:8)   = flip(typecast(obj.Ki, 'uint8'));
            stream(9:12)  = flip(typecast(obj.Kd, 'uint8'));
            stream(13:16) = flip(typecast(obj.time, 'uint8'));
            stream(17:20) = flip(typecast(obj.value, 'uint8'));
            stream(21:24) = flip(typecast(obj.value_max, 'uint8'));
            stream(25:28) = flip(typecast(obj.value_min, 'uint8'));
            stream(29:32) = flip(typecast(obj.error, 'uint8'));
            stream(33:36) = flip(typecast(obj.integral, 'uint8'));
        end
    end
    
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('CLass')
                z = PidType;
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = PidType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(PidType,varargin{:});
            end
        end
    end
end