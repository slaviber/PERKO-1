using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;

namespace ConsoleApplication9
{
    class Program
    {
        static void Main(string[] args)
        {
            for (int i = 1; i <= 160; ++i)
            {
                double res = (i * 1.6 * Math.PI) / 1.5;
                res *= 512.0 / 536.0;
                int r2 = 512 / (int)((res / 2) * 2);
                r2 = 512 / r2;
                for (int a = 1; a < 10; ++a)
                {
                    int check = (int)Math.Pow(2, a);
                    if (check >= r2)
                    {
                        if ((r2 - Math.Pow(2, a - 1)) < (Math.Pow(2, a) - r2))
                        {
                            Console.WriteLine(Math.Pow(2, a - 1));
                        }
                        else
                        {
                            Console.WriteLine(Math.Pow(2, a));
                        }
                        break;
                    }
                }
            }

            Image img = Image.FromFile("C:/Users/Administrator/Desktop/TUES.png");
            Bitmap bmp = new Bitmap(img);
            List<byte> pdata = new List<byte>();
            List<Tuple<int, int, int, int>> pixpos = new List<Tuple<int, int, int, int>>();
            for (int i = 0; i < 80; ++i)
            {
                double d = Math.Asin(i / 80.0);
                d = Math.Cos(d)*79;
                d *= 2;
                int res = (int)Math.Ceiling(d / 4) * 4;
                int rbegin = 160 / 2 - res / 2;
                Console.WriteLine(res + " " + rbegin);
                for (int a = rbegin; a < res+rbegin; a += 4)
                {
                    Color[] p = new Color[4];
                    Color[] p2 = new Color[4];
                    int[] Y = new int[4];
                    int[] U = new int[4];
                    int[] V = new int[4];
                    int[] Y2 = new int[4];
                    int[] U2 = new int[4];
                    int[] V2 = new int[4];

                    for(int j = 0; j < 4; ++j){
                        p[j] = bmp.GetPixel(80+i, a + j);
                        Y[j] = ((66 * p[j].R + 129 * p[j].G + 25 * p[j].B + 128) >> 8) + 16;
                        U[j] = ((-38 * p[j].R - 74 * p[j].G + 112 * p[j].B + 128) >> 8) + 128;
                        V[j] = ((112 * p[j].R - 94 * p[j].G - 18 * p[j].B + 128) >> 8) + 128;
                        p2[j] = bmp.GetPixel(79 - i, a + j);
                        Y2[j] = ((66 * p2[j].R + 129 * p2[j].G + 25 * p2[j].B + 128) >> 8) + 16;
                        U2[j] = ((-38 * p2[j].R - 74 * p2[j].G + 112 * p2[j].B + 128) >> 8) + 128;
                        V2[j] = ((112 * p2[j].R - 94 * p2[j].G - 18 * p2[j].B + 128) >> 8) + 128;
                    }

                    int Umid = (U[0] + U[1] + U[2] + U[3]) / 4;
                    int Vmid = (V[0] + V[1] + V[2] + V[3]) / 4;

                    int Umid2 = (U2[0] + U2[1] + U2[2] + U2[3]) / 4;
                    int Vmid2 = (V2[0] + V2[1] + V2[2] + V2[3]) / 4;

                    pixpos.Add(new Tuple<int, int, int, int>(80 + i, a+0, pdata.Count, 2));
                    pixpos.Add(new Tuple<int, int, int, int>(80 + i, a+1, pdata.Count, 3));
                    pixpos.Add(new Tuple<int, int, int, int>(80 + i, a+2, pdata.Count, 4));
                    pixpos.Add(new Tuple<int, int, int, int>(80 + i, a+3, pdata.Count, 5));
                    pdata.Add((byte)Umid);
                    pdata.Add((byte)Vmid);
                    pdata.Add((byte)Y[0]);
                    pdata.Add((byte)Y[1]);
                    pdata.Add((byte)Y[2]);
                    pdata.Add((byte)Y[3]);

                    pixpos.Add(new Tuple<int, int, int, int>(79 - i, a + 0, pdata.Count, 2));
                    pixpos.Add(new Tuple<int, int, int, int>(79 - i, a + 1, pdata.Count, 3));
                    pixpos.Add(new Tuple<int, int, int, int>(79 - i, a + 2, pdata.Count, 4));
                    pixpos.Add(new Tuple<int, int, int, int>(79 - i, a + 3, pdata.Count, 5));
                    pdata.Add((byte)Umid2);
                    pdata.Add((byte)Vmid2);
                    pdata.Add((byte)Y2[0]);
                    pdata.Add((byte)Y2[1]);
                    pdata.Add((byte)Y2[2]);
                    pdata.Add((byte)Y2[3]);

                    for (int j = 0; j < 4; ++j)
                    {
                        int C = Y[j] - 16;
                        int D = 128 - 128;
                        int E = 128 - 128;
                        int R = (298 * C + 409 * E + 128) >> 8;
                        int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                        int B = (298 * C + 516 * D + 128) >> 8;
                        if (R < 0) R = 0;
                        if (R > 255) R = 255;
                        if (G < 0) G = 0;
                        if (G > 255) G = 255;
                        if (B < 0) B = 0;
                        if (B > 255) B = 255;

                        bmp.SetPixel(80 + i, a + j, Color.FromArgb(255, R, G, B));

                        C = Y2[j] - 16;
                        D = 128 - 128;
                        E = 128 - 128;
                        R = (298 * C + 409 * E + 128) >> 8;
                        G = (298 * C - 100 * D - 208 * E + 128) >> 8;
                        B = (298 * C + 516 * D + 128) >> 8;
                        if (R < 0) R = 0;
                        if (R > 255) R = 255;
                        if (G < 0) G = 0;
                        if (G > 255) G = 255;
                        if (B < 0) B = 0;
                        if (B > 255) B = 255;
                        bmp.SetPixel(79 - i, a + j, Color.FromArgb(255, R, G, B));
                    }




                   // Console.WriteLine(c.R + " " + c.G + " " + c.B);
                }

            }

            List<int> index = new List<int>();
            List<int> offset = new List<int>();
            for (int i = 0; i < 512; ++i)
            {
                double x = Math.Cos((Math.PI / 256.0) * i);
                double y = Math.Sin((Math.PI / 256.0) * i);


                for (int a = 39; a >= 0; --a)
                {
                    int a2 = a * 2 + 1;
                    int px2 = (int)(x * a2);
                    int py2 = (int)(y * a2);
                    bmp.SetPixel(80 - px2, 80 + py2, Color.FromArgb(a*6, 0, 0, 0));
                    bool found = false;
                    Tuple<int, int, int, int> t = null;
                    for (int f = 0; f < pixpos.Count; ++f)
                    {
                       t = pixpos.ElementAt(f);
                        if (t.Item1.Equals(80 - px2) && t.Item2.Equals(80 + py2))
                        {
                            index.Add(t.Item3);
                            offset.Add(t.Item4);
                            found = true;
                            break;
                        }
                    }
                    if (!found) throw new Exception();
                    //int color = ((pdata.ElementAt(t.Item3+t.Item4) - 16) * 298) >> 8;
                    //bmp.SetPixel(80 - px2, 80 + py2, Color.FromArgb(255, color, color, color));

                }

                for (int a = 0; a < 40; ++a)
                {
                    int a1 = a * 2;
                    int px = (int)(x * a1);
                    int py = (int)(y * a1);
                    bmp.SetPixel(80 + px, 80 - py, Color.FromArgb(a*6, 0, 0, 0));
                    bool found = false;
                    Tuple<int, int, int, int> t = null;
                    for (int f = 0; f < pixpos.Count; ++f)
                    {
                        t = pixpos.ElementAt(f);
                        if (t.Item1.Equals(80 + px) && t.Item2.Equals(80 - py))
                        {
                            index.Add(t.Item3);
                            offset.Add(t.Item4);
                            found = true;
                            break;
                        }
                    }
                    if (!found) throw new Exception();
                    //int color = ((pdata.ElementAt(t.Item3+t.Item4) - 16) * 298) >> 8;
                    //bmp.SetPixel(80 + px, 80 - py, Color.FromArgb(255, color, color, color));
                }

            }

            bmp.Save("C:/Users/Administrator/Desktop/TUESfail.bmp");
            File.WriteAllLines("C:/Users/Administrator/Desktop/index.txt", index.ConvertAll<string>(x => x.ToString()));

            System.IO.StreamWriter file = new System.IO.StreamWriter("C:/Users/Administrator/Desktop/index.txt");
            file.WriteLine("const uint16_t index[" + index.Count + "] = {");
            for (int i = 0; i < index.Count; ++i)
            {
                file.WriteLine(index.ElementAt(i) + ", ");
            }
            file.WriteLine("};");
            file.Close();

            file = new System.IO.StreamWriter("C:/Users/Administrator/Desktop/offset.txt");
            file.WriteLine("const uint8_t offset[" + offset.Count + "] = {");
            for (int i = 0; i < offset.Count; ++i)
            {
                file.WriteLine(offset.ElementAt(i) + ", ");
            }
            file.WriteLine("};");
            file.Close();

            file = new System.IO.StreamWriter("C:/Users/Administrator/Desktop/buf.txt");
            file.WriteLine("uint8_t hbuf[" + pdata.Count + "] = {");
            for (int i = 0; i < pdata.Count; ++i)
            {
                file.WriteLine(pdata.ElementAt(i) + ", ");
            }
            file.WriteLine("};");
            file.Close();
        }
    }
}