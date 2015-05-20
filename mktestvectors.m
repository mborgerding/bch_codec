
N=31;K=21; PPoly = [1 0 1 0 0 1]; PPolyDec=PPoly*(2.^[0:5]');  T=2;
GPoly = bchpoly(N,K);

ntv = K;
W=zeros(ntv,N);
W(1,:) = bchenco( [1 zeros(1,K-1) ] , N , K,GPoly,'end');

for i=2:size(W,1)
    %W(i,:) = bchenco( circshift( [1 zeros(1,K-1) ]',i-1)' , N , K,GPoly,'end'); % slide a single set bit along the message
    W(i,:) = W(i-1,[N 1:(N-1)]); % create circular shifts of the valid codeword (which are also valid codewords by properties of BCH)
end

M = W(:,1:K);
E = W(:,(K+1):N);

fprintf('unsigned int prim_poly=%d;\n' , PPolyDec);
fprintf('const int N=%d;\n' , N );
fprintf('const int K=%d;\n' , K );
fprintf('const int ntrials=%d;\n' , ntv );
fprintf('int m=%d;\n' , log2(N+1) );
fprintf('int t=%d;\n' , T );

fprintf('char msgbits[ntrials][K+1] = {');
for i=1:ntv
    fprintf('"'); fprintf('%d',M(i,:));fprintf('", ');
end
fprintf('};\n ');

fprintf('char eccbits[ntrials][N-K+1] = {');
for i=1:ntv
    fprintf('"'); fprintf('%d',E(i,:));fprintf('", ');
end
fprintf('};\n ');


