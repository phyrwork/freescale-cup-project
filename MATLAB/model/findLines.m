function [ lines ] = findLines( edges, type )
%findLines Version 3:

    % SETUP
    lines = [];
    status = 0;
    
    % start first line
    if length(edges) == 0 || edges(1).type ~= type
        % insert a virtual edge
        line.start.pos = 0;
        line.start.type = 2;
        
        status = 1;
        i = 1;
    else
        line.start = edges(1);
        
        status = 1;
        i = 2;
    end
    
    % ANALYSE
    while i <= length(edges)
        
        % search for a matching edge
        while i <= length(edges) && ...
              edges(i).type == line.start.type; % exclude matching edges
        
              i = i + 1; % continue
        end
        
        if i <= length(edges) % if index is still valid
            
            % complete the line
            line.finish = edges(i);
            line.width = line.finish.pos - line.start.pos;
            
            lines = [lines, line];
            line = [];
            status = 0;
        end
        
        % start search for a new line
        while i <= length(edges) && ...
              edges(i).type ~= type % edge
          
            i = i + 1; % continue
        end
        
        if i <= length(edges) % if index is still valid
            
            % start the line
            line.start = edges(i);
            
            status = 1;
        end
    end
    
    % finish last line
    if status == 1 && line.start.pos < 128
        line.finish.pos = 128;
        line.finish.type = 2;
        line.width = line.finish.pos - line.start.pos;
        
        lines = [lines, line];
        line = [];
        status = 0;
    end
end

