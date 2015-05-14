%
% Hex Space
%

classdef EveHexSpace
  
  properties    
    
    N = 0;
    root_triSide = 0;
    xOffset = 0;
    ax_dirs = [];
    ax_vecs = [];
    
  end

 methods
   
   % ctor
   function obj = hex_basis(N)
     % calc size and origin of largest triangle (aka root)
     % note: root encloses N x N square
     obj.N = N;
     a = 1/sqrt(3) * N;
     obj.root_triSide = N + 2*a;
     obj.xOffset = -a;
     
     % hex axes dirs
     obj.ax_dirs(1) = pi/6;
     obj.ax_dirs(2) = ax_dir(1) - pi/3;
     obj.ax_dirs(3) = t(1) + pi/3;
     
     % hex axis vectors
     obj.ax_vecs = zeros(3,2);
     for dir = 1:3
       a = obj.ax_dirs(dir);
       obj.ax_vecs(dir,:) = [ cos(a) sin(a) ];
     end;
     
   end
      
   function m = pointToHexMatrix(obj, x, y, numLevels)
     % calc hex matrix
     x = x - obj.xOffset;
     triSide = obj.root_triSide;
     m = zeros(3, numLevels);
     for level = 1:numLevels
       m(:, level) = pointToHexVector(x, y, triSide);
       triSide = triSide / 2;
     end
   end

   function v = pointToHexVector(obj, x, y, triSide)
     
     % calc projections
     period = triSide * cos(t(1));
     proj = zeros([size(x) 3]);
     strips = zeros(size(proj));
     for dir = 1:3
       axProj = (x * obj.ax_vecs(dir, 1)) + (y * obj.ax_vecs(dir, 2));
       proj(:, :, dir) = axProj;
       strips(dir) = floor(axProj/(period/2));
     end;
     
     % determine hex field vector
     v = [0 0 0];
     hexStrips = floor((strips + 1) / 2);
     
     if (mod(hexStrips(1) + hexStrips(2) + hexStrips(3), 2) == 0)
       v(0) = 1;
     end
     
   end
   
   
   function createHexFieldImages(x, y, numLevels)
     [x, y] = meshgrid(0:N-1);
   end
   
 end
end

%{
    if (APPLY_THRESHOLD)
        f(f>=0) = 1;
        f(f<0) = -1;
    end;
    if (CLIP_MODE & FLAG_CLIP_POSITIVE)
        f(f<0)=0;
    elseif (CLIP_MODE & FLAG_CLIP_NEGATIVE)
        f(f>0)=0;
    end

% multiply different scales

figure(2);
colormap(cmap);

m = ones(N);

for level = 1:numLevels
    f = field{level,4};
    m = m .* f;
end;
    field = strings{1}.*field{2}.*field{3};
    field{level, 4} = f;
    
    subplot(4, numLevels, sub2ind([numLevels 4], level, 4));
    imagesc(f);
    axis xy square off;
    
    % next level
    freq = freq * 2;

imagesc(m);
axis xy square off;
    freq = 1 / period;
        w = 2 * pi * freq;
    
%}
    