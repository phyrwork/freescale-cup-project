classdef EdgeType
    %EdgeType
    
    properties
        type   = EdgeTypeType.empty;
        pos    = uint8.empty;
        P_dPos = single.empty;
        P_edge = single.empty;
    end

    methods
        % EdgeType constructor
        function obj = EdgeType(stream)
            % Check input type
            obj.type      = stream(1);
            obj.pos       = stream(2);
            obj.P_dPos(1) = typecast(flip(stream(3:6)), 'single');
            obj.P_dPos(2) = typecast(flip(stream(7:10)), 'single');
            obj.P_edge(1) = typecast(flip(stream(11:14)), 'single');
            obj.P_edge(2) = typecast(flip(stream(15:18)), 'single');
        end
        
        function stream = pack(obj)
            % pack properties into stream
            stream(1) = obj.type;
            stream(2) = obj.pos;
            stream(3:6) = flip(typecast(obj.P_dPos(1), 'uint8'));
            stream(7:10) = flip(typecast(obj.P_dPos(2), 'uint8'));
            stream(11:14) = flip(typecast(obj.P_edge(1), 'uint8'));
            stream(15:18) = flip(typecast(obj.P_edge(2), 'uint8'));
        end
    end
end