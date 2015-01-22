function plotEdges( image )
%plotEdges
    
    % find edges
    edges = findEdges(image);
    
    % prep scatter
    sy = image(edges);
    
    plot(1:128,image); %plot image
    hold on
    scatter(edges, sy); %plot edges
    hold off
end

