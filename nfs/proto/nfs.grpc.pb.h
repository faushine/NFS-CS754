// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: nfs.proto
#ifndef GRPC_nfs_2eproto__INCLUDED
#define GRPC_nfs_2eproto__INCLUDED

#include "nfs.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace NFS {

class nfsServer final {
 public:
  static constexpr char const* service_full_name() {
    return "NFS.nfsServer";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status call(::grpc::ClientContext* context, const ::NFS::request& request, ::NFS::response* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>> Asynccall(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>>(AsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>> PrepareAsynccall(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>>(PrepareAsynccallRaw(context, request, cq));
    }
    class experimental_async_interface {
     public:
      virtual ~experimental_async_interface() {}
      virtual void call(::grpc::ClientContext* context, const ::NFS::request* request, ::NFS::response* response, std::function<void(::grpc::Status)>) = 0;
      virtual void call(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::NFS::response* response, std::function<void(::grpc::Status)>) = 0;
    };
    virtual class experimental_async_interface* experimental_async() { return nullptr; }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>* AsynccallRaw(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::NFS::response>* PrepareAsynccallRaw(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status call(::grpc::ClientContext* context, const ::NFS::request& request, ::NFS::response* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::NFS::response>> Asynccall(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::NFS::response>>(AsynccallRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::NFS::response>> PrepareAsynccall(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::NFS::response>>(PrepareAsynccallRaw(context, request, cq));
    }
    class experimental_async final :
      public StubInterface::experimental_async_interface {
     public:
      void call(::grpc::ClientContext* context, const ::NFS::request* request, ::NFS::response* response, std::function<void(::grpc::Status)>) override;
      void call(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::NFS::response* response, std::function<void(::grpc::Status)>) override;
     private:
      friend class Stub;
      explicit experimental_async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class experimental_async_interface* experimental_async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class experimental_async async_stub_{this};
    ::grpc::ClientAsyncResponseReader< ::NFS::response>* AsynccallRaw(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::NFS::response>* PrepareAsynccallRaw(::grpc::ClientContext* context, const ::NFS::request& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_call_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_call() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requestcall(::grpc::ServerContext* context, ::NFS::request* request, ::grpc::ServerAsyncResponseWriter< ::NFS::response>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_call<Service > AsyncService;
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_call() {
      ::grpc::Service::experimental().MarkMethodCallback(0,
        new ::grpc::internal::CallbackUnaryHandler< ::NFS::request, ::NFS::response>(
          [this](::grpc::ServerContext* context,
                 const ::NFS::request* request,
                 ::NFS::response* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   return this->call(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithCallbackMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  typedef ExperimentalWithCallbackMethod_call<Service > ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_call() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_call() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void Requestcall(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_call() {
      ::grpc::Service::experimental().MarkMethodRawCallback(0,
        new ::grpc::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this](::grpc::ServerContext* context,
                 const ::grpc::ByteBuffer* request,
                 ::grpc::ByteBuffer* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   this->call(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithRawCallbackMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void call(::grpc::ServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_call : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_call() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::NFS::request, ::NFS::response>(std::bind(&WithStreamedUnaryMethod_call<BaseClass>::Streamedcall, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_call() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status call(::grpc::ServerContext* context, const ::NFS::request* request, ::NFS::response* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status Streamedcall(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::NFS::request,::NFS::response>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_call<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_call<Service > StreamedService;
};

}  // namespace NFS


#endif  // GRPC_nfs_2eproto__INCLUDED
