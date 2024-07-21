using ParameterInfo.JsonUtils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ParameterInfo;
using MarketData;
using HedgingEngine.GrpcClient;
using HedgingEngine.Hedger;
using System.Text.Json.Serialization;
using System.Text.Json;

//main qui permet de lancer le programme
namespace HedgingEngine
{
    public class HedgingConsole
    {

        public static void Main(string[] args)
        {
            if (args.Length != 3) {
                Console.WriteLine("Exactly three arguments required.");
                Console.WriteLine("Usage: ./HedgingEngine test_params.json MarketData.csv portfolio.json");
             }
            string jsonTestParamString = File.ReadAllText(args[0]);
            TestParameters testParams = JsonIO.FromJson(jsonTestParamString);
         
            List<DataFeed> dataFeeds = MarketDataReader.ReadDataFeeds(args[1]);
            PricerClient pricerClient = new PricerClient(testParams);
            Hedger.Hedger hedger = new Hedger.Hedger(dataFeeds[0], pricerClient);
            hedger.Hedge(dataFeeds);
            string outputDataString = JsonIO.ToJson(hedger.OutputDataList);
            File.WriteAllText(args[2], outputDataString);
        }
    }
}
