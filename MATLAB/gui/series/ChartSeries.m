classdef ChartSeries
    %ChartSeries
    
    properties
        view   = ChartView.empty;
        record = TftpRecord.empty;
        plot   = double.empty;
        label  = char.empty;
        style  = struct;
    end
    
    methods
        % ChartSeries constructor
        function obj = ChartSeries(view, attribute, varargin)
            
            % parse constructor inputs
            p = inputParser;
            addRequired(p,  'view');
            addRequired(p,  'attribute');
            addParameter(p, 'label',  char.empty, @ischar);
            addParameter(p, 'style',  'line'    , @ischar);
            addParameter(p, 'color',  [0 0.4470 0.7410]);
            addParameter(p, 'marker', 'o',        @ischar);
            parse(p, view, attribute, varargin{:});
            
            % fetch record
            obj.view   = p.Results.view;
            obj.record = obj.view.session.findAttribute(attribute);
            
            % store other parameters
            obj.label = p.Results.label;
            
            obj.style.graph = p.Results.style; % style parameters
            obj.style.color = p.Results.color;
            obj.style.marker = p.Results.marker;
        end
        
        % setup a new series with x,y data
        function obj = setup(obj, x, y)
            
            % return if no data
            if isempty(x) || isempty(y)
                return;
            end
            
            hold(obj.view.axes, 'on'); % hold other series on these axes
            
            % now add the new series
            switch (obj.style.graph)
                
                % line graphs
                case 'line'
                    obj.plot = plot(...
                                obj.view.axes, x, y,...
                                'DisplayName', obj.label, ...  % store the legend entry in the plot
                                'Color', obj.style.color ...
                                    );
                    
                % scatter grapdfadasdhs
                case 'scatter'
                    obj.plot = scatter(...
                                obj.view.axes, x, y,...
                                'DisplayName', obj.label, ...
                                'MarkerEdgeColor', obj.style.color, ...
                                'Marker', obj.style.marker ...
                                );
            end
            
            hold(obj.view.axes, 'off');
            
            % update the legend
            legend('-DynamicLegend');
        end
        
        % draw the series
        function obj = draw(obj, x, y)
            
            % create the series if it doesn't exist
            if isempty(obj.plot)   
                
                obj = obj.setup(x, y);
                
            % otherwise just replace the data
            else
                set(obj.plot, 'XData', x, 'YData', y);
            end 
        end
        
        % update the series data
        function obj = refresh(obj)
            
            % get latest data from record
            [x,y] = obj.record.latest(obj.view.period);
            
            % draw the series
            obj = obj.draw(x, y);
        end
        
        % get min/max values
        function val = xmin(obj)
            val = min(get(obj.plot, 'XData'));
        end
        function val = xmax(obj)
            val = max(get(obj.plot, 'XData'));
        end
        function val = ymin(obj)
            val = min(get(obj.plot, 'YData'));
        end
        function val = ymax(obj)
            val = max(get(obj.plot, 'YData'));
        end       
    end
end
