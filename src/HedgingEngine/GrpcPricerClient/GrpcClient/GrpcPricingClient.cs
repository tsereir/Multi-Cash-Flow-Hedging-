using Google.Protobuf.WellKnownTypes;
using Grpc.Net.Client;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using ParameterInfo.MathModelDescriptions;
using System;
using System.Net.Http;
using System.Threading.Channels;
using static GrpcPricing.Protos.GrpcPricer;

//classe qui fait la liaison entre le client et le serveur
namespace HedgingEngine.GrpcClient
{
    class GrpcPricingClient
    {
        public GrpcChannel Channel { get; set; }
        public GrpcPricerClient GrpcClient { get; set; }

        public GrpcPricingClient(string serverAddress = "http://localhost:50051")
        {
            var httpHandler = new HttpClientHandler();
            httpHandler.ServerCertificateCustomValidationCallback =
            HttpClientHandler.DangerousAcceptAnyServerCertificateValidator;
            Channel = GrpcChannel.ForAddress(serverAddress, new GrpcChannelOptions { HttpHandler = httpHandler });
            GrpcClient = new GrpcPricer.GrpcPricerClient(Channel);
     
        }

    }
}