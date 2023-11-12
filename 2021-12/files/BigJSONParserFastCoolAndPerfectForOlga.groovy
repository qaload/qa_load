import com.google.gson.Gson
import com.google.gson.GsonBuilder

public class Util{
          class Data {
			String showcaseId
			int version
			List<Category> categories
		}
		class Category {
			String id
			String name
			String imageUrl
			List<String> products
			String parentId
			int rank
		}
    public static ArrayList productsGenerate(String body){
    		def gson = new Gson()
    		def input = gson.fromJson( body, Data )
    		def count = new Random().nextInt(7 - 3) + 3
    		def loop=0
    		def result = new ArrayList()
    		while (loop<count) {
			def category = input.categories[new Random().nextInt(input.categories.size())]
			if (category.products.size()>0) {
				result.add(category.products[new Random().nextInt(category.products.size())])
				loop=loop+1
			}
				
    		}
    		return result	
    }
}

props.put("Util",Util)

