close all;
clear all;

#is slow in octave
#may need to run it a few times for it to learn the ff sound

pkg load signal

%load training sounds
x=audioread('../matlab/oh_from_off.wav');
sig_oh_from_off= resample(x,80,441);
clear x;
x=audioread('../matlab/ff_from_off.wav');
sig_ff_from_off= resample(x,80,441);
clear x;
x=audioread('../matlab/oh_from_on.wav');
sig_oh_from_on= resample(x,80,441);
clear x;
x=audioread('../matlab/in_from_on.wav');
sig_in_from_on= resample(x,80,441);
clear x;
x=audioread('../matlab/blank.wav');
sig_blank= resample(x,80,441);
clear x;

% figure;specgram(sig_oh_from_on,2^12,8000,blackman(128),120)
% figure;specgram(sig_in_from_on,2^12,8000,blackman(128),120)

%on
[oh_from_on,oh_from_on_f,oh_from_on_t]=specgram(sig_oh_from_on,256,8000,hann(128),120);
oh_from_on=log(abs(oh_from_on));
[in_from_on,in_from_on_f,in_from_on_t]=specgram(sig_in_from_on,256,8000,hann(128),120);
in_from_on=log(abs(in_from_on));

%off
[oh_from_off,oh_from_off_f,oh_from_off_t]=specgram(sig_oh_from_off,256,8000,hann(128),120);
oh_from_off=log(abs(oh_from_off));
[ff_from_off,ff_from_off_f,ff_from_off_t]=specgram(sig_ff_from_off,256,8000,hann(128),120);
ff_from_off=log(abs(ff_from_off));

%blank
[blank,blank_f,blank_t]=specgram(sig_blank,256,8000,hann(128),120);
blank=log(abs(blank));


%number of tests
epoch_count=1000000;

%results buffer
costs=zeros([epoch_count 1]);

%setup sizes
m=256/2;
k=400;
n=4;

%setup stepsize
alpha=0.0005;

%fill rand numbers
W1=2*(rand([k m])-0.5);
W2=2*(rand([n k])-0.5);
B1=2*(rand([k 1])-0.5);
B2=2*(rand([n 1])-0.5);

for epoch=1:epoch_count
    
    sel=randi(5,1);
    if(sel==1)
        X=oh_from_off(:,randi(numel(oh_from_off_t),1));
        Y_exp=[1,0,0,0]';
    end;
    if(sel==2)
        X=ff_from_off(:,randi(numel(ff_from_off_t),1));
        Y_exp=[0,1,0,0]';
    end;
    if(sel==3)
        X=oh_from_on(:,randi(numel(oh_from_on_t),1));
        Y_exp=[1,0,0,0]';
    end;
    if(sel==4)
        X=in_from_on(:,randi(numel(in_from_on_t),1));
        Y_exp=[0,0,1,0]';
    end;
    if(sel==5)
        X=blank(:,randi(numel(blank_t),1));
        Y_exp=[0,0,0,1]';
    end;
    
    %calculate solution
    H=sigmoid(W1*X+B1);
    Y=sigmoid(W2*H+B2);
    
    %calc cost
    J=0.5*sum((Y_exp-Y).^2);
    
    if(mod(epoch,floor(epoch_count/100))==0)
      fprintf('epoch=%d/%d\n',epoch,epoch_count);
    end
    
    costs(epoch)=J;
    
    %calc slope
    delta_2=(Y-Y_exp).*divsigmoid(W2*H+B2);
    delta_1=(W2'*delta_2).*divsigmoid(W1*X+B1);
    dJW2=delta_2*H';
    dJW1=delta_1*X';
    dJB2=delta_2;
    dJB1=delta_1;
    
    %update weights
    B2=B2-alpha*dJB2;
    B1=B1-alpha*dJB1;
    W2=W2-alpha*dJW2;
    W1=W1-alpha*dJW1;
    
    % end;
    
end;

figure;plot(smooth(costs,5001));

%uncomment these to save the matricies
%save weights and offsets for others to use
%savepath='E:\git\build-jvoice-64bit_static-Release\release';
% save([savepath,'\W1.txt'],'W1','-ascii')
% save([savepath,'\W2.txt'],'W2','-ascii')
% save([savepath,'\B1.txt'],'B1','-ascii')
% save([savepath,'\B2.txt'],'B2','-ascii')

%training done

%testing on new real life data

%load on and off file
x=audioread('../matlab/on_and_off.wav');
sig_on_and_off= resample(x,80,441);
clear x;
[on_and_off,on_and_off_f,on_and_off_t]=specgram(sig_on_and_off,256,8000,hann(128),120);
on_and_off=log(abs(on_and_off));

%put through function
sols=zeros([numel(on_and_off_t) n])*nan;
for k=1:numel(on_and_off_t)
    
    %load some signal
    X=on_and_off(:,k);
    
    %calculate solution
    H=sigmoid(W1*X+B1);
    Y=sigmoid(W2*H+B2);
    sols(k,:)=Y';
end;

%show results
figure;plot(on_and_off_t,sols);legend('o','f','n','\_');xlabel('time (s)');title('unheard speech');

%smooth it 
figure;
plot(on_and_off_t,smooth(sols(:,1),51));hold on;
plot(on_and_off_t,smooth(sols(:,2),51));
plot(on_and_off_t,smooth(sols(:,4),51));
plot(on_and_off_t,smooth(sols(:,3),51));
hold off;
legend('o','f','n','\_');
xlabel('time (s)');
title('unheard speech smoothed');

%turn into text
pr_o=smooth(sols(:,1),51);
pr_f=smooth(sols(:,2),51);
pr_n=smooth(sols(:,3),51);
pr_blank=smooth(sols(:,4),51);
thres=0.7;
lastidx=-1;
for k=2:numel(pr_blank)
    
   [max_prob,idx]=max([pr_o(k),pr_f(k),pr_n(k),pr_blank(k)]);
   if(isnan(max_prob))
       continue;
   end;
   if(max_prob<thres)
       continue;
   end;
   if(lastidx==idx)
       continue;
   end;
   switch idx
       case 1
            fprintf('o');
       case 2
           fprintf('ff');
       case 3
           fprintf('n');
       case 4
           fprintf('\n');
   end;
   lastidx=idx;
    

end;

%plot just the ones over threshold 
idx=find(pr_o<thres);
pr_o(idx)=nan;
idx=find(pr_n<thres);
pr_n(idx)=nan;
idx=find(pr_f<thres);
pr_f(idx)=nan;
idx=find(pr_blank<thres);
pr_blank(idx)=nan;
figure;
plot(on_and_off_t,pr_o);hold on;
plot(on_and_off_t,pr_f);
plot(on_and_off_t,pr_n);
plot(on_and_off_t,pr_blank);hold off;
legend('o','f','n','\_');
xlabel('time (s)');
title('unheard speech smoothed over threshold');

