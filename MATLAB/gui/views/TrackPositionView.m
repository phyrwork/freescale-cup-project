classdef TrackPositionView < LineChartView
    %TrackPositionView
    
    properties
        % attribute = char.empty; % attribute id
        % session   = []; % handle of session view is attached to
        % record    = []; % handle of record view is attached to
        % hfig      = []; % handle of figure (layout) view belongs to
        % haxis     = []; % handle of axis
        % atitle    = char.empty; % axis title
        % axlabel   = char.empty; % x-axis label
        % aylabel   = char.empty; % y-axis label
        % hplot     = []; % plot handle
        % period    = single.empty; % chart period (s)
    end
    
    methods
        % TrackPositionView constructor
        function obj = TrackPositionView(session, attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'session');
            addRequired (p, 'attribute', @isattribute);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            % addParameter(p, 'title', char.empty, @ischar);
            % addParameter(p, 'xlabel', char.empty, @ischar);
            % addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, session, attribute, varargin{:});
            
            % initialise LineChartView
            obj = obj@LineChartView(...
                p.Results.session,...
                p.Results.attribute,...
                'figure', p.Results.figure,...
                'position', p.Results.position,... 
                'title', ['Track position: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Perceived displacement (px)',...
                'ylabel', 'Time (s)'...
            );
        end
    end
    
    % View update methods
    methods
        % update chart - overload update@LineChartView
        function obj = update(obj)
            % fetch and prepare data
            [times,values] = obj.record.latest(obj.period);
            
            % draw chart - swap x/y to "rotate" chart
            obj = obj.draw(values, times);
            ylim(obj.haxis, [times(1), times(end)]);
            xlim(obj.haxis, [-64, 64]); % adjust y-axis limits
        end
    end
    
end
