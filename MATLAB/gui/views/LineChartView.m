classdef LineChartView < AxisView
    %LineChartView

    methods
        % LineChartView constructor
        function obj = LineChartView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, attribute, varargin{:});
            
            % initialise AxisView
            obj = obj@AxisView(...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
        end
        
        % draw chart
        function obj = draw(obj, times, values)
            % select axis
            obj.select();
            
            % plot data
            plot(times, values);

            % apply superclass draw() components
            obj = draw@AxisView(obj);
        end
    end
end

