package me.com.grpc

import android.os.Bundle
import android.support.v4.view.PagerAdapter
import android.support.v7.app.AppCompatActivity
import android.support.v7.widget.LinearLayoutManager
import android.support.v7.widget.RecyclerView
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.google.gson.Gson
import com.jakewharton.rxbinding.view.RxView.clicks
import io.grpc.ManagedChannelBuilder
import io.grpc.examples.article.nano.Article
import io.grpc.examples.article.nano.ArticleStoreGrpc.newStub
import io.grpc.examples.article.nano.ArticlesForPeriodRequest
import io.grpc.stub.StreamObserver
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import kotlinx.android.synthetic.main.item_article.view.*
import kotlinx.android.synthetic.main.viewpager.*
import retrofit2.Call
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import retrofit2.http.GET
import rx.Observable
import rx.android.schedulers.AndroidSchedulers
import rx.schedulers.Schedulers
import java.util.concurrent.TimeUnit

class MainActivity : AppCompatActivity() {

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.viewpager)

    viewPager.adapter = object : PagerAdapter() {
      override fun instantiateItem(container: ViewGroup?, position: Int): Any? {
        return LayoutInflater.from(container!!.context).inflate(R.layout.activity_main, container, false).apply {
            container.addView(this)
          if(position == 0) this.bindToGRPC()
          if(position == 1) this.bindToJson()
        }
      }

      override fun isViewFromObject(view: View?, `object`: Any?): Boolean = view == `object`

      override fun getCount(): Int = 2

      override fun getPageTitle(position: Int): CharSequence? = when(position) {
        0 -> "GRPC"
        1 -> "JSON"
        else -> TODO()
      }
    }

    grpcTab.setOnClickListener { viewPager.setCurrentItem(0) }
    jsonTab.setOnClickListener { viewPager.setCurrentItem(1) }
  }
}

fun View.bindToGRPC() {
  val adapter = ArticleAdapter()
  articleList.layoutManager = LinearLayoutManager(context)
  articleList.adapter = adapter

  clicks(btnHitServer)
          .throttleFirst(1, TimeUnit.SECONDS)
          .switchMap({
            requestWithNewConnection(addressInput.text.toString(), portInput.text.toString().toInt(),
                    inputCategoryId.text.toString().toInt(), startTimestamp.text.toString(), endTimestamp.text.toString())
                    .onBackpressureBuffer(10000)
                    .map { it.id.toLong() to it.title }
                    .onErrorReturn { RecyclerView.NO_ID to it.message }
                    .scan(emptyList<Pair<Long, String>>(), { a, b -> a + b })
                    .subscribeOn(Schedulers.newThread())
          })
          .subscribeOn(AndroidSchedulers.mainThread())
          .observeOn(AndroidSchedulers.mainThread())
          .subscribe({
            adapter.items = it
            adapter.notifyDataSetChanged()
          })

}

val gson = Gson()

fun View.bindToJson() {
  val adapter = ArticleAdapter()
  articleList.layoutManager = LinearLayoutManager(context)
  articleList.adapter = adapter

  clicks(btnHitServer)
          .throttleFirst(1, TimeUnit.SECONDS)
          .observeOn(Schedulers.newThread())
          .switchMap({
            Observable.just(
                    Retrofit.Builder().baseUrl("http://10.11.9.156:5000/")
                            .addConverterFactory(GsonConverterFactory.create(gson))
                            .build()
                            .create(ArticleStoreApi::class.java)
                            .request()
                            .execute().body()
            ).map { it.articles.map { -1L to "${it}" } }
          })
          .subscribeOn(AndroidSchedulers.mainThread())
          .observeOn(AndroidSchedulers.mainThread())
          .subscribe({
            adapter.items = it
            adapter.notifyDataSetChanged()
          })

}

fun requestWithNewConnection(ip: String, port: Int,
  categoryId: Int, startTimestamp: String, endTimestamp: String) = Observable.create<Article> { subscriber ->

  val req = ArticlesForPeriodRequest().apply {
    this.categoryId = categoryId
    this.startTimestamp = startTimestamp
    this.endTimestamp = endTimestamp
  }

  newStub(ManagedChannelBuilder.forAddress(ip, port).usePlaintext(true).build())
    .articlesForPeriod(req, object : StreamObserver<Article> {
      override fun onNext(value: Article) {
        value.let { subscriber.onNext(it) }
      }

      override fun onCompleted() {
        subscriber.onCompleted()
      }

      override fun onError(t: Throwable?) {
        subscriber.onError(t!!)
      }
    })
}

class ArticleAdapter() : RecyclerView.Adapter<RecyclerView.ViewHolder>() {
  var items: List<Pair<Long, String>> = emptyList()

  override fun getItemCount(): Int = items.count()

  override fun onBindViewHolder(holder: RecyclerView.ViewHolder?, position: Int) {
    holder?.itemView?.articleTitle?.text = items[position].second
  }

  override fun getItemId(position: Int): Long = items[position].first

  override fun onCreateViewHolder(parent: ViewGroup?, viewType: Int) =
    object : RecyclerView.ViewHolder(
      LayoutInflater.from(parent?.context).inflate(R.layout.item_article, parent, false)) {

    }
}

interface ArticleStoreApi {
  @GET("/") fun request(): Call<ArticleResponse>
}

data class ArticleResponse(
        val articles: List<List<Object>> = emptyList()
)