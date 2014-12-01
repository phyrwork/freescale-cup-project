classdef LineChartView < AxisView
    %LineChartView
    
    properties
        % attribute = char.empty; % attribute id
        % session   = []; % handle of session view is attached to
        % record    = []; % handle of record view is attached to
        % hfig      = []; % handle of figure (layout) view belongs to
        % haxis     = []; % handle of axis
        % atitle    = char.empty; % axis title
        % axlabel   = char.empty; % x-axis label
        % aylabel   = char.empty; % y-axis label
        hplot       = []; % plot handle
        period      = single.empty; % chart period (s)
    end

    methods
        % LineChartView constructor
        function obj = LineChartView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'session');
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            addParameter(p, 'period', 20, @issingle); % default period = 20s
            parse(p, session, attribute, varargin{:});
            
            % initialise AxisView
            obj = obj@AxisView(...
                p.Results.session,...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
            obj.period = p.Results.period;
        end
    end
    
    % View update methods
    methods
        % draw chart
        function obj = draw(obj, x, y)
            % draw
            if (isempty(obj.hplot)) % initialise plot
                obj.hplot = plot(obj.haxis, x, y);
                obj = label(obj);
            else % replace plot data
                set(obj.hplot, 'XData', x, 'YData', y);
            end
        end
        
        % update chart
        function obj = update(obj)
            % fetch data
            [x,y] = obj.record.latest(obj.period);
            
            % draw chart
            obj = obj.draw(x, y);
        end
    end
    
end

