function result = isattribute( var )
%isattribute GuiView isattribute validation function. Checks is either char
%string or cell array of char strings

    result = true;
    if iscell(var)
        for i = 1:length(var)
            if ~ischar(var{i})
                result = false;
            end
        end
    else
        if ~ischar(var)
            result = false;
        end
    end

end

