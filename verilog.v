module mmm(
input input1,
input input2,
output output5,
output output7
);
wire gatewire0;
wire gatewire1;
wire gatewire2;
wire gatewire3;
wire gatewire4;
wire gatewire5;
wire gatewire6;
wire gatewire7;
wire gatewire8;
wire gatewire9;
wire gatewire10;
assign gatewire0=input1;
assign gatewire1=input1;
assign gatewire3=input2;
assign gatewire4=input2;
assign output5=gatewire5;
assign output7=gatewire7;
assign gatewire2= !gatewire1;
assign gatewire10= !(gatewire3%gatewire2);
assign gatewire9= !(gatewire4%gatewire0);
assign gatewire5= !(gatewire9%gatewire8);
 assign gatewire6=gatewire5;
assign gatewire7= !(gatewire6%gatewire10);
 assign gatewire8=gatewire7;
endmodule