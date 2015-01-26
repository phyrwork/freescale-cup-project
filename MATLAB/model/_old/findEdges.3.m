function edges = findEdges( image )
%findEdges Version 3:

    % SETTINGS
    ry_t = 600;
    dy_t = 350;
    
    % SETUP
    dy = derivative(image);
    edges = [];
    i = 2;
    ry = 0;
    
    while i < length(dy)
        
        disp(['Searching at ', num2str(i), '...'])
        
        % first identify the starts of possible edges
        if abs( dy(i) ) > dy_t % if pixel dy above threshold
            
            disp(['Pixel ', num2str(i), ' meets dy threshold.'])
            
            start = i - 1;
            
            % advance until inflection point or dy below threshold
            while abs( dy(i) ) > dy_t && ~(dy(i) > 0 && dy(i-1) < 0) || ...
                  (dy(i) < 0 && dy(i-1) > 0)
                
                % accumulate dy
                ry = ry + dy(i);
                i = i + 1;
                
            end % now at position after end of last candidate region
            
            disp(['End of region at pixel', num2str(i), '.'])
            
            % decide what to do with candidate
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
                
            end % still at position after end of last candidate region...
            
            ry = 0; % reset height
            
        else
            i = i + 1;
        end
    end
end


