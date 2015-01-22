function edges = findEdges( image )
%findEdges Version 3.1:

    % SETTINGS
    ry_t = 600;
    dy_t = 350;
    
    % SETUP
    dy = derivative(image);
    edges = [];
    i = 2;
    
    % ANALYSE
    while i < length(dy)
                
        % reset
        ry = 0;
        
        % first search for start of a region
        while ~(abs( dy(i) ) > dy_t ) && ...
              i < length(dy) % search for dy above threshold
            % advance
            i = i + 1;
        end % now at start of candidate region
        start = i;
        
        % next search for end of a possible region
        while abs( dy(i) ) > dy_t && ... %
              ~( (dy(i) > 0 && dy(i-1) < 0) || (dy(i) < 0 && dy(i-1) > 0) ) && ...
              i < length(dy)
            % accumulate dy and advance
            ry = ry + dy(i);
            i = i + 1;
        end % now at end of a candidate region
        
        % test candidate
        if abs( ry ) > ry_t
            % candidate meets requirements, so choose side of region
            if ry > 0
                % rising edge, choose start of region
                pos = start;
            else
                % falling edge, choose end of region
                pos = i - 1;
            end

            % save edge
            edges = [edges, pos];
        end
        
        i = i + 1; % advance
    end
end


