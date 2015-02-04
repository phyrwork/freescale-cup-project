classdef LinescanEdgesSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = LinescanEdgesSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    'label', 'Detected Edges (Modelled)', ...
                    'style', 'scatter', ...
                    'color', 'red', ...
                    'marker', '*' ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [~,y] = obj.record.peek;
            
            % identify edges
            e = findEdges(y, 250, 600);
            x = [];
            for i = 1:length(e)
                x = [x, e(i).pos];
            end
            
            % get values of y and indicies x
            y = y(x);
            
            % draw the series
            obj = obj.draw(x, y);
        end
    end
end
