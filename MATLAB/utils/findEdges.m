function [ edges ] = findEdges( image )
%findEdges Experimental edge detection for FRDM-TFC
    
    % array of edge objects
    edges = [];
    
    % start first edge
    edge.start.pos = 1;
    edge.start.dy  = 0;
    edge.sum   = 0;
    
    % scan image
    last_dy = 0;
    
    for i = 2:length(image);
        
        % check dy
        this    = image(i);
        last    = image(i - 1);
        this_dy = this - last;
        
        % detect change in direction
        if (this_dy > 0 && last_dy < 0) || (this_dy < 0 && last_dy > 0)
            
            % save edge and start a new one
            edge.finish.pos = i - 1;
            edge.finish.dy  = this_dy;
            edges = [edges, edge];
            edge.start.pos = i;
            edge.sum = 0;
        end
        
        % sum dy and prep next
        edge.sum = edge.sum + this_dy;
        last_dy = this_dy;
    end
    
    % finish final edge
    edge.finish.pos = i;
    edge.finish.dy  = this_dy;
    edges = [edges, edge];
    
    % find qualifying edges
    ind = [];
    
    for i = 1:length(edges)
        
        % choose edges over summation threshold
        if abs(edges(i).sum) > 1000
            ind = [ind, i];
        end
    end
    
    % complete and return qualifying edges
    edges = edges(ind);
    
    for i = 1:length(edges)
    
        % choose steepest side of edge
        if edges(i).start.dy > edges(i).finish.dy
            edges(i).pos = edges(i).start.pos;
        else
            edges(i).pos = edges(i).finish.pos;
        end
    end
end

