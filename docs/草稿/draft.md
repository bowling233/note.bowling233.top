# 零散笔记

## API 工具

- **[OpenAPI](https://learn.openapis.org/)** 是一个标准规范，用于描述 REST API，让人和机器都能理解接口，不限编程语言。OpenAPI 前身就是 Swagger 规范，被捐赠给 Linux 基金会而成。
- **Swagger** 是一套工具（如 Swagger UI、Editor、Codegen），基于 OpenAPI 规范，帮助设计、文档化和生成代码。

### OpenAPI

描述 OpenAPI 的文件称为 OpenAPI 文档（OpenAPI Document，OAD），通常是 YAML 格式。[OpenAPI Map](https://openapi-map.apihandyman.io/) 给出了 OAD 的所有字段的说明。一个最简单的 OAD 如下：

```yaml
openapi: 3.0.0
info:
  title: Sample API
  version: 1.0.0
paths:
    /hello:
        get:
        summary: Hello World
        description: Returns a hello world message.
        responses:
            '200':
            description: A hello world message
            content:
                application/json:
                schema:
                    type: object
                    properties:
                    message:
                        type: string
                        example: Hello, world!
```

## S3 对象存储

名称来源：Simple Storage Service，最早是亚马逊的产品。

基本概念：

- 桶：可以理解为顶层文件夹，桶内可以有子文件夹。
- 对象：存储在桶内

## CORS

现代浏览器强制 SOP（Same Origin Policy），即同源策略，限制网页仅能对同源的资源进行请求。这样可以防止恶意网站跨域请求用户的敏感信息。

跨域资源共享（Cross-Origin Resource Sharing，CORS）是一种浏览器机制，允许网页从不同的源（域名、协议或端口）请求资源。CORS 通过 HTTP 头部来控制哪些源可以访问资源。
