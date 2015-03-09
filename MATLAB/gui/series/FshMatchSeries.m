classdef FshMatchSeries < ChartSeries
    %ChartSeries
    
    methods
        % ChartSeries constructor
        function obj = FshMatchSeries(view, attribute, varargin)
            
            % initialise series
            obj = obj@ChartSeries(...
                    view, attribute, ...
                    'label', 'New match', ...
                    'style', 'line', ...
                    'color', 'green' ...
                  );
            
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get most recent image from record
            [x, s] = obj.record.latest(obj.view.period);
            s = [s(:).P.fsh];
            
            % draw the series
            obj = obj.draw(x, s);
        end
    end
end
