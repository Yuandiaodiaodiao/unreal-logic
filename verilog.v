module mmm(
input input1,
input input2,
output output5,
output output4
);
wire gatewire0;
wire gatewire1;
wire gatewire2;
wire gatewire3;
wire gatewire4;
wire gatewire5;
assign gatewire1=input1;
assign gatewire3=input1;
assign gatewire0=input2;
assign gatewire2=input2;
assign output5=gatewire5;
assign output4=gatewire4;
assign gatewire5=gatewire0|gatewire1;
assign gatewire4=gatewire3&gatewire2;
endmodule