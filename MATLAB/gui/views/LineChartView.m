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
            addParameter(p, 'xlim', 'auto');
            addParameter(p, 'ylim', 'auto');
            addParameter(p, 'period', 20, @issingle); % default period = 20s
            addParameter(p, 'legend', []);
            parse(p, session, attribute, varargin{:});
            
            % initialise AxisView
            obj = obj@AxisView(...
                p.Results.session,...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel,...
                'xlim', p.Results.xlim,...
                'ylim', p.Results.ylim,...
                'legend', p.Results.legend...
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
                obj.hplot = plot(obj.haxis, x, y); % returns one handle for each line (i.e. if x is 2-dimensional)
                obj = label(obj);
                if (~isempty(obj.hplot)) % only call legend if plot exists to surpress warnings
                    obj.clegend('on');
                end
            else % replace plot data
                for h = 1:length(obj.hplot) % update one line at a time
                    set(obj.hplot, 'XData', x(h,:), 'YData', y); % replace one row of data
                end
            end
        end
        
        % update chart
        function obj = update(obj)
            % if no data nothing to do, return
            if (obj.record.rsize < 1)
                return;
            end
            
            % fetch data
            [x,y] = obj.record.latest(obj.period);
            
            % draw chart
            obj = obj.draw(x, y);
            xlim(obj.haxis, [x(1), x(end)]); % adjust y-axis limits
        end
    end
    
end

