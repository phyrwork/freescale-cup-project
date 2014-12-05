classdef ProfilerEventTypeType < uint8
    %ProfilerEventTypeType
    
    enumeration
        start      (10)
        stop       (20)
        checkpoint (30)
        period     (40)
    end
   
    methods (Static)
        % zeros support
        function z = zeros(varargin)
            if (nargin == 0)
                % For zeros('Class')
                z = ProfilerEventTypeType(255);
            elseif any([varargin{:}] <= 0)
                % For zeros with any dimension <= 0
                z = ProfilerEventTypeType.empty(varargin{:});
            else
                % For zeros(m,n,...,'Class')
                % Use property default values
                z = repmat(ProfilerEventTypeType(255),varargin{:});
            end
        end
    end
end