function [ edges ] = findEdges( y )
%findEdges Experimental edge detection for FRDM-TFC

    % SETTINGS
    threshold = 600;
    
    % SETUP
    dy = y(2:end) - y(1:end-1); % find image derivative
    dy = [0;dy]; % sync y, dy
    
    % initialise edges array
    edges = [];
    
    % start first edge
    edge = [];
    edge.start  = 1;
    edge.height = 0;
    
    % ANALYSE
    for i = 2:length(dy)
        
         % detect inflection points
         if (dy(i) > 0 && dy(i-1) < 0) || (dy(i) < 0 && dy(i-1) > 0)
             
             % filter qualifying edges
             if abs(edge.height) > threshold
                 
                 % complete edge
                 edge.finish = i - 1;
                 edge.pos = edge.start;
                 
                 % pinpoint edge location
                 for c = edge.start:edge.finish
                     
                     % look for steepest point in region
                     if abs(dy(c)) > abs(dy(edge.pos))
                         
                         % assign new edge position
                         edge.pos = c;
                     end
                 end
                 
                 % save edge
                 edges = [edges, edge];
             end
             
             % start new edge
             edge = [];
             edge.start = i;
             edge.height = 0;
         end
         
         % sum height
         edge.height = edge.height + dy(i);
    end
    
    % filter qualifying edges
    if abs(edge.height) > threshold

         % complete edge
         edge.finish = i - 1;
         edge.pos = edge.start;

         % pinpoint edge location
         for c = edge.start:edge.finish

             % look for steepest point in region
             if abs(dy(c)) > abs(dy(edge.pos))

                 % assign new edge position
                 edge.pos = c;
             end
         end
         
         % save edge
         edges = [edges, edge];
    end
end

