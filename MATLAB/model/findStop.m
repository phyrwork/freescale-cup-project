function result = findStop( edges )
%findStop

    % SETTINGS
    width_mu = 15;
    width_sig = 5;
    width_Pt = 0.6;
    gap_mu = 10;
    gap_sig = 5;
    gap_Pt = 0.6;
    
    % ANALYSE
    lines = findLines(edges, 1); % find black lines
    
    % test pairs of lines for match
    result = false;
    for i = 2:length(lines)
        
        % test separation
        gap = floor(lines(i).start.pos - lines(i-1).finish.pos);
        P = getProb(gap, gap_mu, gap_sig);
        if P < gap_Pt
            continue;
        end
        
        % test widths
        P1 = getProb(lines(i-1).width, width_mu, width_sig);
        P2 = getProb(lines(i).width, width_mu, width_sig);
        if P1 < width_Pt || P2 < width_Pt
            continue;
        end
        
        result = true;
        return;
    end
end

