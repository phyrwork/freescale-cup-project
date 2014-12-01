classdef MotorCurrentView < LineChartView
    %MotorCurrentView
    
    methods
        % MotorCurrentView constructor
        function obj = MotorCurrentView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            % addParameter(p, 'title', char.empty, @ischar);
            % addParameter(p, 'xlabel', char.empty, @ischar);
            % addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, attribute, varargin{:});
            
            % initialise LineChartView
            obj = obj@LineChartView(...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', ['Motor current: ',attribute],...
                'xlabel', 'Time (s)',...
                'ylabel', 'Current (A)'...
            );
        end
        
        % draw chart 
        % function obj = draw@LineChartView(obj, times, values)
    end
    
end

