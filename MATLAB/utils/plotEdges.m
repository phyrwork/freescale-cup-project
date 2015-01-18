function plotEdges( image )
%plotEdges
    
    % find edges
    edges = findEdges(image);
    
    % prep scatter
    sx = [];
    for i = 1:length(edges)
        sx = [sx, edges(i).pos];
    end
    sy = image(sx);
    
    plot(1:128,image); %plot image
    hold on
    scatter(sx, sy); %plot edges
    hold off
end

