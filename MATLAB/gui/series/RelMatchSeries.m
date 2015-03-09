classdef RelMatchSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = RelMatchSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    'label', 'Relative match', ...
                    'style', 'line', ...
                    'color', 'red' ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [x, s] = obj.record.latest(obj.view.period);
            s = [s(:).P.rel];
            
            % draw the series
            obj = obj.draw(x, s);
        end
    end
end
