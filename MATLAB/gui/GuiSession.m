classdef GuiSession
    %GuiSession
    
    properties
        session = [];
        views = {};
    end
    
    methods
        %GuiSession constructor
        function obj = GuiSession(session)
            % set parameters
            obj.session = session;
            
            % define layout
            % -example -
            % obj = obj.addViews(...
            %     LinescanView(obj.session, 'linescan0', 'figure', 1, 'position', [3,1,1]),...
            %     MotorCurrentView(obj.session, 'i_rl', 'figure', 1, 'position', [3,1,2]),...
            %     MotorCurrentView(obj.session, 'i_rr', 'figure', 1, 'position', [3,1,3])...
            % );
        end
        
        % update
        function obj = update(obj)
            % draw all views
            for i = 1:length(obj.views)
                obj.views{i} = obj.views{i}.update();
            end
        end
    end
    
    % addView methods
    methods
        % addViews
        function obj = addViews(obj, varargin)
            % for all arguments
            for i = 1:length(varargin)
                % check is drawable
                m = methods(varargin{i});
                m = find(strcmp(m, 'update'), 1, 'first');
                if (isempty(m))
                    disp('This object is not drawable (i.e. its draw() method is missing.');
                    continue;
                end
                
                % add to cell array of views
                obj.views{length(obj.views) + 1} = varargin{i};
            end
        end
        
        % addLayout
        function obj = addLayouts(obj, varargin)
            % for all arguments
            for i = 1:length(varargin)
                % get a unique figure handle
                fh = findall(0,'type','figure');
                if (isempty(fh))
                    fh = 0;
                end
                nf = max(fh(find(fh == fix(fh))));

                % call layout function
                layout = varargin{i};
                obj = layout(obj, nf + 1);

                % layout function contains appropriate calls to addViews to
                % generate the desired window
            end
        end
    end
end

