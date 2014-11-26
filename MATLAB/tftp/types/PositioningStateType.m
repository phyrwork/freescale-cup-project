classdef PositioningStateType < uint8
    %PositioningStateType
    
    enumeration
        full          (0)
        partial_left  (10) 
        partial_right (20)
        none          (255)
    end
end

