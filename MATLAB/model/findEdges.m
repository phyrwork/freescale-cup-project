function edges = findEdges( image, dy_t, ry_t )
%findEdges Version 3.1:
    
    % SETUP
    image = double(image);
    dy = derivative(image);
    dy = filtfilt([0.25,0.25,0.25,0.25],1,dy);
    %dy = derivative(image);
    
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
                edge.pos = start;
                edge.type = 0; % rising edge
            else
                % falling edge, choose end of region
                edge.pos = i - 1;
                edge.type = 1; % rising edge
            end

            % save edge
            edges = [edges, edge];
            edge = [];
        end
        
        i = i + 1; % advance
    end
end


