using ICSproj.BL.Models;

namespace ICSproj.App.Messages
{
    public class AddedMessage<T> : Message<T>
        where T : IModel
    {
    }
}
