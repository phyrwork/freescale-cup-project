classdef AxisView < GuiView
    %ChartView Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % attribute = char.empty; % attribute id
        % fig       = []; % handle of figure (gui element) view belongs to
        haxis       = []; % handle of axis
        atitle      = char.empty; % axis title
        axlabel     = char.empty; % x-axis label
        aylabel     = char.empty; % y-axis label
    end
    
    methods
        % AxisView constructor
        function obj = AxisView(attribute, varargin)
            % parse input
            p = inputParser;
            addRequired (p, 'attribute', @ischar);
            addParameter(p, 'figure', double.empty);
            addParameter(p, 'position', double.empty, @isnumeric);
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, attribute, varargin{:});
            
            % initialise GuiView
            obj = obj@GuiView(...
                p.Results.attribute,...
                'figure', p.Results.figure...
            );
            
            % initialise AxisView
            obj.haxis = p.Results.position;
            if (isempty(obj.haxis)) % no handle/subplot pos. specified
                obj.haxis = subplot(1,1,1); % set a default
            elseif (length(obj.haxis) == 3) % subplot pos. specified
                obj.haxis = subplot(... % convert to handle
                    obj.haxis(1),...
                    obj.haxis(2),...
                    obj.haxis(3)...
                );
            end
            
            obj = obj.label(...
                'title', p.Results.title,...
                'xlabel', p.Results.xlabel,...
                'ylabel', p.Results.ylabel...
            );
        end
        
        % select axis
        function select(obj)
            % select@GuiView; % focus view figure
            axes(obj.haxis);  % focus view axis
        end
        
        % label axis
        function obj = label(obj, varargin)
            % parse input
            p = inputParser;
            addParameter(p, 'title', char.empty, @ischar);
            addParameter(p, 'xlabel', char.empty, @ischar);
            addParameter(p, 'ylabel', char.empty, @ischar);
            parse(p, varargin{:});
            
            % apply labels
            if (~isempty(p.Results.title)) % if label supplied
                obj.atitle = p.Results.title;  % store new label
            end
            title(obj.haxis, obj.atitle); % apply stored label
            
            if (~isempty(p.Results.xlabel))
                obj.axlabel = p.Results.xlabel;
            end
            xlabel(obj.haxis, obj.axlabel);
            
            if (~isempty(p.Results.ylabel))
                obj.aylabel = p.Results.ylabel;
            end
            ylabel(obj.haxis, obj.aylabel);
        end
    end
    
end

