using ICSproj.BL.Models;

namespace ICSproj.App.Messages
{
    public class UpdateMessage<T> : Message<T>
        where T : IModel
    {
    }
}
