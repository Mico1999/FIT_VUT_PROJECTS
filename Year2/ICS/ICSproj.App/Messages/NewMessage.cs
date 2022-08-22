using ICSproj.BL.Models;

namespace ICSproj.App.Messages
{
    public class NewMessage<T> : Message<T>
        where T : IModel
    {
    }
}
