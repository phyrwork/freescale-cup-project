classdef LinescanImageSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = LinescanImageSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    'label', 'Intensity', ...
                    'style', 'line', ...
                    'color', 'blue' ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [~,y] = obj.record.peek;
            y = transpose(y);
            x = 1:1:length(y);
            
            % draw the series
            obj = obj.draw(x, y);
        end
    end
end
