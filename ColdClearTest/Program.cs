using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using ColdClear;

namespace ColdClearTest
{
    class NextGenerator
    {
        public NextGenerator()
        {
            bag = new CCPiece[7];
            for (int i = 0; i < 7; i++)
            {
                bag[i] = (CCPiece)i;
            }
            index = 7;
            r = new Random();
        }
        public CCPiece Next()
        {
            if (index == 7)
            {
                bag = bag.OrderBy(x => r.Next()).ToArray();
                index = 0;
            }
            return bag[index++];
        }

        Random r;
        CCPiece[] bag;
        int index;
    }
    class Program
    {
        static string GetLocation(byte[] x, byte[] y)
        {
            return $"{x[0]},{y[0]} {x[1]},{y[1]} {x[2]},{y[2]} {x[3]},{y[3]}";
        }

        static void PrintMovePlan(MovePlans mp)
        {
            Console.WriteLine($"Node: {mp.Move.Nodes}, Depth: {mp.Move.Depth}, Rank: {mp.Move.OriginalRank}");
            Console.WriteLine($"UseHold: {mp.Move.Hold}");
            Console.WriteLine($"Location: {GetLocation(mp.Move.ExpectedX, mp.Move.ExpectedY)}");
            Console.WriteLine($"Plans: {mp.Plans.Length}");
            foreach (var plan in mp.Plans)
            {
                Console.WriteLine($"Piece: {plan.Piece}, Tspin: {plan.TSpin}");
                Console.WriteLine($"Location: {GetLocation(plan.ExpectedX, plan.ExpectedY)}");
                for (int j = 0; j < 4; j++)
                {
                    int cl = plan.ClearedLines[j];
                    if (cl == -1) break;
                    Console.WriteLine($"Cleared line: {cl}");
                }
            }
        }
        static void Main(string[] args)
        {
            var options = CCOptions.Default();
            options.threads = 2;
            options.max_nodes = 100000;
            var weights = CCWeights.Default();
            NextGenerator g = new NextGenerator();
            CCPieceBag bag = CCPieceBag.Full();
            bool[] field = new bool[400];
            using (var cc = new Bot(options, weights, field, bag, CCPiece.CC_None, false, 0))
            {
                for (int i = 0; i < 10; i++)
                {
                    cc.AddNextPiece(g.Next());
                }
                Thread.Sleep(100);
                for (int i = 0; i < 20; i++)
                {
                    PrintMovePlan(cc.GetNextMoveAndPlans(0));
                    cc.AddNextPiece(g.Next());
                    Thread.Sleep(100);
                }
                cc.Reset(field, false, 0);
                PrintMovePlan(cc.GetNextMoveAndPlans(0));
            }
        }
    }
}
