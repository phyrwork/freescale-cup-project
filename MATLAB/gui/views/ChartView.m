classdef ChartView < handle
    %ChartView
    
    properties
        session = TftpSession.empty;
        figure  = double.empty;
        axes    = double.empty;
        series  = {};                % series objects
        period  = single.empty;      % chart period (seconds)
        title   = char.empty;
        xlabel  = char.empty;
        ylabel  = char.empty;
        xtop    = struct;
        xbot    = struct;
        ytop    = struct;
        ybot    = struct;
    end
    
    methods
        % constructor
        function obj = ChartView(session, varargin)
            
            % parse input
            p = inputParser;
            p.KeepUnmatched = true;
            addRequired (p, 'session');
            addParameter(p, 'figure',   double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title',    char.empty,   @ischar);
            addParameter(p, 'xlabel',   char.empty,   @ischar);
            addParameter(p, 'ylabel',   char.empty,   @ischar);
            addParameter(p, 'period',   20,           @isnumeric);
            addParameter(p, 'xtopu',    double.empty, @isnumeric); % xtop upper limit
            addParameter(p, 'xtopl',    double.empty, @isnumeric); % xtop lower limit
            addParameter(p, 'xtopc',    double.empty, @isnumeric); % xtop constant
            addParameter(p, 'xbotu',    double.empty, @isnumeric); % xbot upper limit
            addParameter(p, 'xbotl',    double.empty, @isnumeric); % xbot lower limit
            addParameter(p, 'xbotc',    double.empty, @isnumeric); % xbot constant
            addParameter(p, 'ytopu',    double.empty, @isnumeric); % ytop upper limit
            addParameter(p, 'ytopl',    double.empty, @isnumeric); % ytop lower limit
            addParameter(p, 'ytopc',    double.empty, @isnumeric); % ytop constant
            addParameter(p, 'ybotu',    double.empty, @isnumeric); % ybot upper limit
            addParameter(p, 'ybotl',    double.empty, @isnumeric); % ybot lower limit
            addParameter(p, 'ybotc',    double.empty, @isnumeric); % ybot constant
            parse(p, session, varargin{:});
            
            % session handle
            obj.session = p.Results.session; 
            
            % figure handle
            if isempty(p.Results.figure)
                obj.figure = obj.newFigure();
            else
                obj.figure = p.Results.figure;
            end
            
            % initialise axes handle 
            if isempty(p.Results.position)         % no handle/subplot pos. specified
                
                obj.axes = subplot(1,1,1);         % set a default
                
            elseif length(p.Results.position) == 3 % subplot pos. specified
                
                figure(obj.figure);                % select figure
                obj.axes = subplot(...             % get axes handle
                    p.Results.position(1),...
                    p.Results.position(2),...
                    p.Results.position(3)...
                );
            end 
            
            % initialize chart period
            obj.period = p.Results.period;
            
            % label axes
            obj = obj.label(...
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
        
            % store axes limits
            obj.xtop.upper = p.Results.xtopu;
            obj.xtop.lower = p.Results.xtopl;
            obj.xtop.const = p.Results.xtopc;
            
            obj.xbot.upper = p.Results.xbotu;
            obj.xbot.lower = p.Results.xbotl;
            obj.xbot.const = p.Results.xbotc;
            
            obj.ytop.upper = p.Results.ytopu;
            obj.ytop.lower = p.Results.ytopl;
            obj.ytop.const = p.Results.ytopc;
            
            obj.ybot.upper = p.Results.ybotu;
            obj.ybot.lower = p.Results.ybotl;
            obj.ybot.const = p.Results.ybotc;
        end
        
        % label axis
        function obj = label(obj, varargin)
            
            % parse input
            p = inputParser;
            addParameter(p, 'title',  char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, varargin{:});
            
            % apply title
            if (~isempty(p.Results.title))   % if label supplied
                obj.title = p.Results.title; % store new label
            end
            title(obj.axes, obj.title);      % apply stored label
            
            % apply x-axis label
            if (~isempty(p.Results.xlabel))
                obj.xlabel = p.Results.xlabel;
            end
            xlabel(obj.axes, obj.xlabel);
            
            % apply y-axis label
            if (~isempty(p.Results.ylabel))
                obj.ylabel = p.Results.ylabel;
            end
            ylabel(obj.axes, obj.ylabel);
        end
        
        % add data series
        function obj = addSeries(obj, series, attribute, varargin)
            
            % call the specified series constructor with the supplied arguments
            obj.series{end + 1} = series(obj, attribute, varargin{:});
        end
        
        % refresh view
        function obj = refresh(obj)
            
            % in turn, refresh each data series
            for i = 1:length(obj.series)
                obj.series{i} = obj.series{i}.refresh;
            end
            
            % set the axes limits
            obj.setLimits;
        end
        
        % set axes limits
        function setLimits(obj)
            
            h = allchild(obj.axes);             % get handles of axes children
            x = get(findobj(h,'-property','XData'), 'XData'); % get XData
            y = get(findobj(h,'-property','YData'), 'YData'); % get YData
            
            % set xtop, xmin
            if iscell(x)
                %xmax = max(cellfun(@(x)max(x(:)), x, 'UniformOutput', false));
                xmax = max(x{1});
                for i = 2:length(x)
                    tmp = max(x{i});
                    if tmp > xmax
                        xmax = tmp;
                    end
                end
                
                %xmin = min(cellfun(@(x)min(x(:)), x, 'UniformOutput', false));
                xmin = min(x{1});
                for i = 2:length(x)
                    tmp = min(x{i});
                    if tmp < xmin
                        xmin = tmp;
                    end
                end
            else
                xmax = max(x);
                xmin = min(x);
            end
            xtop = obj.getLimit(obj.xtop, xmax);
            xbot = obj.getLimit(obj.xbot, xmin);
            
            % set ytop, ymin
            if iscell(y)
                %ymax = max(cellfun(@(y)max(y(:)), y, 'UniformOutput', false));
                ymax = max(y{1});
                for i = 2:length(y)
                    tmp = max(y{i});
                    if tmp > ymax
                        ymax = tmp;
                    end
                end
                
                %ymin = min(cellfun(@(y)min(y(:)), y, 'UniformOutput', false));
                ymin = min(y{1});
                for i = 2:length(y)
                    tmp = min(y{i});
                    if tmp < ymin
                        ymin = tmp;
                    end
                end
            else
                ymax = max(y);
                ymin = min(y);
            end
            ytop = obj.getLimit(obj.ytop, ymax);
            ybot = obj.getLimit(obj.ybot, ymin);
            
            % set limits
            if ~(isempty(xtop) || isempty(xbot)) && xtop ~= xbot
                xlim(obj.axes, [xbot, xtop]);
            end
            if ~(isempty(ytop) || isempty(ybot)) && ytop ~= ybot
                ylim(obj.axes, [ybot, ytop]);
            end
        end
    end
    
    methods (Static)  
        % open a new figure
        function fig = newFigure()

            % find all figure handles
            fh = findall(0,'type','figure');
            if isempty(fh)
                nf = 0; % # is 0 if isempty(fh)
            else
                nf = max([fh.Number]);
            end
            
            fig = figure(nf + 1); % new figure is one above this
        end
        
        % get axis limit
        function limit = getLimit(bound, actual)
            
            % limit is constant
            if ~isempty(bound.const)
                limit = bound.const;
                return;
            end
            
            % set lower
            if ~isempty(bound.lower) && actual < bound.lower
                
                limit = bound.lower;
                
            % set upper
            elseif ~isempty(bound.upper) && actual > bound.upper
                
                limit = bound.upper;
                
            % no limits, fit to data
            else
                limit = actual;
            end
        end
    end
end

