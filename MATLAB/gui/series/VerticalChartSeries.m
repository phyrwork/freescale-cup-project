classdef VerticalChartSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = VerticalChartSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    varargin{:} ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [y,x] = obj.record.latest(obj.view.period);
            
            % draw the series
            obj = obj.draw(x, y);
        end
    end
end
