classdef MotorTorqueView < LineChartView
    %MotorTorqueView
    
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
        % MotorTorqueView constructor
        function obj = MotorTorqueView(session, attribute, varargin)
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
                'title', ['Motor torque: ', strrep(attribute, '_', '\_')],...
                'xlabel', 'Time (s)',...
                'ylabel', 'Torque (Nm)'...
            );
        end
    end
    
end

