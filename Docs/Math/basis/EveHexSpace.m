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
   function obj = EveHexSpace(N)
     % calc size and origin of largest triangle (aka root)
     % note: root encloses N x N square
     obj.N = N;
     a = 1/sqrt(3) * N;
     obj.root_triSide = N + 2*a;
     obj.xOffset = -a;
     
     % hex axes dirs
     obj.ax_dirs(1) = pi/6;
     obj.ax_dirs(2) = obj.ax_dirs(1) - pi/3;
     obj.ax_dirs(3) = obj.ax_dirs(1) + pi/3;
     
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
     period = triSide * cos(obj.ax_dirs(1));
     proj = zeros([size(x) 3]);
     ts = zeros(size(proj));
     for dir = 1:3
       axProj = (x * obj.ax_vecs(dir, 1)) + (y * obj.ax_vecs(dir, 2));
       proj(:, :, dir) = axProj;
       ts(:, :, dir) = floor(axProj/(period/2));
     end;
     
     % determine hex field vector
     hs = floor((ts + 1) / 2);
     L_90 = mod(hs(:,:,1) + hs(:,:,2), 3);
     L_M60 = mod(hs(:,:,1) + hs(:,:,3) + 1, 3);
     
     red = zeros(size(x));
     a = mod(L_90, 3) == 1;
     b = mod(L_M60, 3) == 0;
     red((a+b)==2) = 1;
      
     grn = zeros(size(x));
     a = mod(L_90, 3) == 0;
     b = mod(L_M60, 3) == 1;
     grn((a+b)==2) = 1;
     
     blu = zeros(size(x));
     a = mod(L_90, 3) == 2;
     b = mod(L_M60, 3) == 2;
     blu((a+b)==2) = 1;
     
     v = cat(3,red,grn,blu);
   end
   
   
   function imgs = createHexFieldImages(obj, numLevels)
     [x, y] = meshgrid(0:obj.N-1);
     % calc hex matrix
     x = x - obj.xOffset;
     triSide = obj.root_triSide;
     imgs = cell(numLevels);
     for level = 1:numLevels
       imgs{level} = obj.pointToHexVector(x, y, triSide);
       triSide = triSide / 2;
     end
   end
   
 end
 
 methods (Static = true)
   
   function test()
     
     e = EveHexSpace(512);
     imgs = e.createHexFieldImages(5);
     
     figure(1);
     colormap(gray);
     
     numLevels = size(imgs,1);
     for level = 1:numLevels
       img = imgs{level};
       for fieldNum = 1:3
         subplot(4, numLevels, sub2ind([numLevels 4], level, fieldNum));
         imagesc(img(:,:,fieldNum));
         axis xy square off;
       end
       subplot(4, numLevels, sub2ind([numLevels 4], level, 4));
       m = max(img(:));
       image(img/m);
       axis xy square off;
     end
   end
  
 end  % static methods
 
end % class

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
    