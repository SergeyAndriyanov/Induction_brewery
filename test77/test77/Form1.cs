using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Diagnostics;
using System.Threading;

namespace test77
{
    public partial class Form1 : Form
    {
        //----------------------------------------------------------------------------------------------------------------//
        private readonly int[] wCRCTable =  {
            0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
            0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
            0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
            0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
            0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
            0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
            0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
            0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
            0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
            0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
            0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
            0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
            0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
            0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
            0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
            0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
            0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
            0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
            0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
            0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
            0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
            0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
            0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
            0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
            0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
            0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
            0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
            0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
            0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
            0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
            0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
            0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };
        //----------------------------------------------------------------------------------------------------------------//

        private struct StructDataFromDev
        {
            public byte adress;
            public int u_rectifler;
            public int i_inductor_1;
            public int i_inductor_2;
            public int temperature_inductor_1;
            public int temperature_inductor_2;
            public int temperature_igbt_1;
            public int temperature_igbt_2;
            public byte statusinductor;
            public byte inductor1pwm;
            public byte inductor2pwm;
            public byte pwmfact1;
            public byte pwmfact2;
            public int reserv3;

            public int crc;
        };


        StructDataFromDev strpacketfromdev = new StructDataFromDev();

        byte[] arr = new byte[100];
        byte[] arrin = new byte[100];
        bool recdone = false;
        int numberrecbyte = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {

            serialPort1.PortName = "Com6";
            serialPort1.ReadTimeout = 100;
            serialPort1.Open();
            var th = new Thread(ReceivedDataHeater);
            th.IsBackground = true;
            th.Start();
            timer1.Enabled = true;
        }




        private void button2_Click(object sender, EventArgs e)
        {
            int tempcr = 0;
            arr[0] = (byte)Convert.ToInt32(textBox3.Text);
            if (checkBox1.Checked) { arr[1] = (byte)(arr[1] | 0x01); } else { arr[1] = (byte)(arr[1] & 0xfe); }
            if (checkBox2.Checked) { arr[1] = (byte)(arr[1] | 0x02); } else { arr[1] = (byte)(arr[1] & 0xfd); }
            arr[2] = (byte)Convert.ToInt32(textBox4.Text);
            arr[3] = (byte)Convert.ToInt32(textBox5.Text);
            tempcr = DataCrc(arr, 4);
            arr[4] = (byte)(tempcr >> 8);
            arr[5] = (byte)(tempcr & 0x00ff);
            serialPort1.Write(arr, 0, 6);




        }

        private void ReceivedDataHeater()
        {

            for (; ; )
            {

                try
                {
                    arrin[numberrecbyte] = (byte)serialPort1.ReadByte();
                    numberrecbyte++;
                }
                catch (Exception e)
                {
                    if (numberrecbyte > 1)
                        recdone = true;
                }

                Thread.Sleep(10);
            }

        }



        private void timer1_Tick(object sender, EventArgs e)
        {
            if (recdone)
            {

                int crcr = arrin[numberrecbyte - 2];
                crcr = crcr << 8;
                crcr = crcr | arrin[numberrecbyte - 1];

                if (DataCrc(arrin, numberrecbyte - 2) == crcr)
                {

                    strpacketfromdev.adress = arrin[0];
                    strpacketfromdev.u_rectifler = arrin[1];
                    strpacketfromdev.u_rectifler = (strpacketfromdev.u_rectifler << 8) | arrin[2];
                    strpacketfromdev.i_inductor_1 = arrin[3];
                    strpacketfromdev.i_inductor_1 = (strpacketfromdev.i_inductor_1 << 8) | arrin[4];
                    strpacketfromdev.i_inductor_2 = arrin[5];
                    strpacketfromdev.i_inductor_2 = (strpacketfromdev.i_inductor_2 << 8) | arrin[6];
                    strpacketfromdev.temperature_inductor_1 = arrin[7];
                    strpacketfromdev.temperature_inductor_2 = arrin[8];
                    strpacketfromdev.temperature_igbt_1 = arrin[9];
                    strpacketfromdev.temperature_igbt_2 = arrin[10];
                    strpacketfromdev.statusinductor = arrin[11];
                    strpacketfromdev.inductor1pwm = arrin[12];
                    strpacketfromdev.inductor2pwm = arrin[13];
                    strpacketfromdev.pwmfact1 = arrin[14];
                    strpacketfromdev.pwmfact2 = arrin[15];

                    strpacketfromdev.reserv3 = arrin[16];
                    strpacketfromdev.reserv3 = (strpacketfromdev.reserv3 << 8) | arrin[17];


                    strpacketfromdev.crc = arrin[18];
                    strpacketfromdev.crc = strpacketfromdev.crc << 8;
                    strpacketfromdev.crc = strpacketfromdev.crc | arrin[19];


                    textBox6.Text = strpacketfromdev.adress.ToString();
                    textBox1.Text = strpacketfromdev.u_rectifler.ToString();
                    textBox2.Text = strpacketfromdev.i_inductor_1.ToString();
                    textBox7.Text = strpacketfromdev.i_inductor_2.ToString();
                    textBox8.Text = stainhart(strpacketfromdev.temperature_inductor_1, 0.022485944, -0.012328307, 0.000566819, 3300, 255, 2, 4.982).ToString();
                    textBox9.Text = stainhart(strpacketfromdev.temperature_inductor_2, 0.022485944, -0.012328307, 0.000566819, 3300, 255, 2, 4.982).ToString();

                    textBox11.Text = stainhart(strpacketfromdev.temperature_igbt_1, -0.067764425, 0.016142443, 0.000163046, 5100, 255, 2, 4982).ToString();
                    textBox10.Text = stainhart(strpacketfromdev.temperature_igbt_2, -0.067764425, 0.016142443, 0.000163046, 5100, 255, 2, 4982).ToString();

                    textBox15.Text = strpacketfromdev.pwmfact1.ToString();
                    textBox16.Text = strpacketfromdev.pwmfact2.ToString();


                    checkBox3.Checked = (strpacketfromdev.statusinductor & 0x01) != 0 ? true : false;
                    checkBox4.Checked = (strpacketfromdev.statusinductor & 0x02) != 0 ? true : false;
                    textBox13.Text = strpacketfromdev.inductor1pwm.ToString();
                    textBox12.Text = strpacketfromdev.inductor2pwm.ToString();

                    checkBox7.Checked = (strpacketfromdev.statusinductor & 0x04) != 0 ? true : false;
                    checkBox6.Checked = (strpacketfromdev.statusinductor & 0x08) != 0 ? true : false;

                    checkBox9.Checked = (strpacketfromdev.statusinductor & 0x10) != 0 ? true : false;
                    checkBox8.Checked = (strpacketfromdev.statusinductor & 0x20) != 0 ? true : false;

                    checkBox10.Checked = (strpacketfromdev.statusinductor & 0x40) != 0 ? true : false;
                    checkBox11.Checked = (strpacketfromdev.statusinductor & 0x80) != 0 ? true : false;

                    textBox14.Text = numberrecbyte.ToString();
                   // textBox17.Text = strpacketfromdev.reserv3.ToString();

                    // textBox1.Text = BitConverter.ToString(arrin);
                    //  textBox2.Text = crcr.ToString();
                }





                numberrecbyte = 0;
                recdone = false;
            }
        }

        //--------------------------------------------------//
        //--------------------------------------------------//


        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            timer2.Enabled = ((CheckBox)sender).Checked;
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            int tempcr = 0;
            arr[0] = (byte)Convert.ToInt32(textBox3.Text);
            if (checkBox1.Checked) { arr[1] = (byte)(arr[1] | 0x01); } else { arr[1] = (byte)(arr[1] & 0xfe); }
            if (checkBox2.Checked) { arr[1] = (byte)(arr[1] | 0x02); } else { arr[1] = (byte)(arr[1] & 0xfd); }
            arr[2] = (byte)Convert.ToInt32(textBox4.Text);
            arr[3] = (byte)Convert.ToInt32(textBox5.Text);
            tempcr = DataCrc(arr, 4);
            arr[4] = (byte)(tempcr >> 8);
            arr[5] = (byte)(tempcr & 0x00ff);
            serialPort1.Write(arr, 0, 6);
        }


        private int DataCrc(byte[] nData, int wLength)
        {
            int nTemp;
            int wCRCWord = 0xFFFF;
            int ii = 0;


            while (wLength != 0)
            {
                nTemp = (nData[ii] ^ wCRCWord) & 0x000000ff;
                ii++;
                wCRCWord >>= 8;
                wCRCWord ^= wCRCTable[nTemp];
                wLength--;
            }
            return wCRCWord;
        }//end

        private double stainhart(int ValAdc, double A, double B, double C, double ResistorDiv, int adcmaxcod, double adcmincod, double adcrefV)
        {
            double temp = 0, res = 0;
            if (ValAdc > 0)
            {
                res = adcrefV / (adcrefV / adcmaxcod * ValAdc) * ResistorDiv;
                temp = 1.0 / (A + B * Math.Log10(res) + C * Math.Pow(Math.Abs(Math.Log10(res)), 3));
            }
            return temp;
        }

        private void button3_Click(object sender, EventArgs e)
        {
           textBox17.Text = stainhart(220, 0.022485944, -0.012328307, 0.000566819, 3300, 255, 2, 4.982).ToString();//инд
           // textBox17.Text = stainhart(100, -0.067764425, 0.016142443, 0.000163046, 5100, 255, 2, 4982).ToString();//тран
        }

        //--------------------------------------------------//
    }
}
